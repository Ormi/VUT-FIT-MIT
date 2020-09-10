-- simplify-bkg Core Functions
-- FLP Functional Project
-- Michal Ormos (xormos00)
-- xormos00@stud.fit.vutbr.cz
-- February 2019

module SimplifyBKGCore (getBKG, step1, step2) where

import System.IO
import Data.List
import Data.List.Split
import Data.String

import SimplifyBKGData

--------------------------------------------------------------------------------
--------------------------- Helper functions -----------------------------------
--------------------------------------------------------------------------------
isTerminal :: String -> Bool
isTerminal [x] = x `elem` ['a'..'z']
isTerminal _ = False

isNonTerminal :: String -> Bool
isNonTerminal [x] = x `elem` ['A'..'Z']
isNonTerminal _ = False

removeDuplicates :: [NonTerminal] -> [NonTerminal]
removeDuplicates = foldl (\seen x -> if x `elem` seen
                                      then seen
                                      else seen ++ [x]) []

intersect' :: [NonTerminal] -> [NonTermTerminal] -> [NonTerminal]
intersect' [] _ = []
intersect' (x:xs) l | x `elem` l = x : xs `intersect` l
                   | otherwise = xs `intersect` l

orderBKG :: BKG -> BKG
orderBKG grammar@BKG{
                    nonTerminals = nonTerminals, 
                    terminals = terminals,
                    startState = startState,
                    rules = rules } = BKG newNonterminals newTerminals startState rules
    where
        newNonterminals = checkOrder nonTerminals
        newTerminals = sort terminals

checkOrder :: [NonTerminal] -> [NonTerminal]
checkOrder nonTerminals = if (last nonTerminals == "S") && length nonTerminals > 2
                            then last nonTerminals : init nonTerminals
                            else nonTerminals

allDifferent :: (Ord a) => [a] -> Bool
allDifferent xs = length (nub xs) == length xs

isElementsDuplicated :: [NonTerminal] -> Bool
isElementsDuplicated a = a /= removeDuplicates a

isRuleDuplicated :: [String] -> Bool
isRuleDuplicated a = a /= removeDuplicates a
                                   
prepareGrammar :: String -> BKG
prepareGrammar content = checkDuplicityBKG (parseBKG (checkDuplicatedRules (lines content)))

--------------------------------------------------------------------------------
--------------- Input parsing and grammar validity checks ----------------------
--------------------------------------------------------------------------------
getBKG :: String -> IO BKG
getBKG content = do 
                let finalGrammar = prepareGrammar content
                return finalGrammar

checkDuplicatedRules :: [String] -> [String]
checkDuplicatedRules (a:b:c:d) = if isRuleDuplicated d
                            then error "[ERROR] Duplicated rules"
                            else a:b:c:d

checkDuplicityBKG :: BKG -> BKG
checkDuplicityBKG grammar = BKG checkNonTerminals checkTerminals checkStartState checkRules
    where 
        checkNonTerminals = if isElementsDuplicated (nonTerminals grammar)
                            then error "[ERROR] Duplicated NonTerminal"
                            else nonTerminals grammar
        checkTerminals = if isElementsDuplicated (terminals grammar)
                            then error "[ERROR] Duplicated Terminal"
                            else terminals grammar
        checkStartState = startState grammar
        checkRules = rules grammar

-- parse input Grammar, step by step, nonTerminals, terminals, startState, rules
parseBKG :: [String] -> BKG
parseBKG (nonTerminals:terminals:startState:rules) =
    if null rules then
        error "[ERROR] BKG do not contain rules"
    else
        BKG parseNonTerminals parseTerminals parseStartState parseRules
    where
        -- Non Terminals syntax check
        parseNonTerminals = map processNonTerminal (splitOn "," nonTerminals)
        processNonTerminal :: String -> NonTerminal
        processNonTerminal nonterminal =
            if (length nonterminal == 1) && head nonterminal `elem` ['A'..'Z'] then
                nonterminal
            else 
                error "[ERROR] nonTerminal syntax, bad input"

        -- Terminals syntax check
        parseTerminals = map processTerminal (splitOn "," terminals)
        processTerminal :: String -> Terminal
        processTerminal terminal =
            if (length terminal == 1) && head terminal `elem` ['a'..'z'] then
                terminal
            else
                error "[ERROR] Terminal syntax, bad input"

        -- Start state syntax check
        parseStartState = processStartState startState
        processStartState :: String -> NonTerminal
        processStartState startState =
            if (length startState == 1) && head startState `elem` ['A'..'Z'] then
                startState
            else
                error "[ERROR] Start state syntax, bad input"   
        
        -- Rules syntax check
        parseRules = map processRule rules
        processRule :: String -> Rule
        processRule rule = generateRule (splitOn "->" rule)
            where
                generateRule [leftSide,rightSide] =
                    if leftCheck leftSide && rightCheck rightSide then
                        Rule leftSide rightSide
                    else
                        error "[ERROR] Rules syntax, bad input"
                    where
                        leftCheck :: String -> Bool
                        leftCheck [x] = x `elem` [head y | y <- parseNonTerminals]
                        leftCheck _ = False
                        rightCheck :: String -> Bool
                        rightCheck [] = False
                        rightCheck [x] = (x `elem` ([head x | x <- parseTerminals] ++ [head y | y <- parseNonTerminals])) || (x == '#')
                        rightCheck (x:xs) = x `elem` ([head x | x <- parseTerminals] ++ [head y | y <- parseNonTerminals]) && rightCheck xs
                generateRule _ = error "[ERROR] Rule syntax, rules is empty, bad iput"
parseBKG _ = error "[ERROR] Grammar syntax, bad input"

--------------------------------------------------------------------------------
------------------------------ Core Logic --------------------------------------
--------------------------------------------------------------------------------
-- First part of algorithm, step one
step1 :: String -> IO BKG
step1 content = do
    let preparedGrammar = prepareGrammar content
    let bkgStep1 = applyFirstAlgorithm preparedGrammar
    let orderedBKG = orderBKG bkgStep1
    return orderedBKG

applyFirstAlgorithm :: BKG -> BKG
applyFirstAlgorithm grammar@BKG{
                nonTerminals = nonTerminals, 
                terminals = terminals,
                startState = startState,
                rules = rules } = BKG newNonterminals terminals startState newRules 
    where
        nonTerminalSet = getNewNonTerminalSet grammar []
        newNonterminals = removeDuplicates (nonTerminalSet ++ [startState])   
        newRules = [r | r <- rules, (leftSide r `elem` nonTerminalSet) && checkRightSide nonTerminalSet terminals (rightSide r)]

-- Main iteration, until we get final set of rules
getNewNonTerminalSet :: BKG -> [NonTerminal] -> [NonTerminal]
getNewNonTerminalSet grammar previousNonTerminal =
    if length previousNonTerminal == length newNonTerminal
        then previousNonTerminal 
        else getNewNonTerminalSet grammar (removeDuplicates (previousNonTerminal ++ newNonTerminal))
        where
            newNonTerminal = createNonTerminalSet grammar (createNewRules grammar previousNonTerminal)

createNonTerminalSet :: BKG -> [Rule] -> [NonTerminal]
createNonTerminalSet BKG{nonTerminals = nonTerminals, 
                terminals = terminals,
                startState = startState,
                rules = rules } 
                newSet = [nonterminals | nonterminals <- nonTerminals , any (\rs -> nonterminals == leftSide rs) newSet ] 


createNewRules :: BKG -> [NonTerminal] -> [Rule]
createNewRules BKG{nonTerminals = nonTerminals, 
                terminals = terminals,
                startState = startState,
                rules = rules } 
                previousNonTerminal = [r | r <- rules, checkRightSide previousNonTerminal terminals (rightSide r)]

-- Checking characters from rule right side
checkRightSide :: [NonTerminal] -> [Terminal] -> NonTerminal -> Bool
checkRightSide rightSideTerms terminals
    = foldr
        (\ x ->
           (&&)
             ([x] `elem` terminals ||
                ([x] `elem` rightSideTerms) || (x == '#')))
        True

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- Second part of algorithm, step two
-- Returning grammar without redundant rules
step2 :: String -> IO BKG
step2 content = do
    let preparedGrammar = prepareGrammar content    
    let bkgStep1 = applyFirstAlgorithm preparedGrammar
    let bkgStep2 = applySecondAlgorithm bkgStep1
    let orderedBKG = orderBKG bkgStep2
    return orderedBKG

applySecondAlgorithm :: BKG -> BKG
applySecondAlgorithm grammar@BKG{
                    nonTerminals = nonTerminals, 
                    terminals = terminals,
                    startState = startState,
                    rules = rules } = BKG newNonterminals newTerminals startState newRules
    where
        newRulesSet = getNewRules grammar [startState]
        newNonterminals = intersect' newRulesSet nonTerminals
        newTerminals = intersect' newRulesSet terminals
        newRules = checkRightSideSet rules newRulesSet

-- Main iteration, until we get final set of rules
getNewRules :: BKG -> [NonTerminal] -> [NonTerminal]  
getNewRules grammar@BKG{nonTerminals = nonTerminals, 
                terminals = terminals,
                startState = startState,
                rules = rules } 
                previousRules = 
    if previousRules == newRulesSet
        then previousRules 
        else getNewRules grammar newRulesSet
        where
            newRulesSet = removeDuplicates (previousRules ++ createNewRulesSet' grammar previousRules)

-- Creating set of terminals and nonTerminals
-- on or other is added to final set
createNewRulesSet' :: BKG -> [NonTerminal] -> [NonTerminal]
createNewRulesSet' BKG{nonTerminals = nonTerminals, 
                terminals = terminals,
                startState = startState,
                rules = rules } 
                previousRules = [newSet | newSet <- termNontermSet, any (\rs -> not (null (newSet `intersect` rightSide rs))) checkRightSide] 
    where
        checkRightSide = checkRightSideSet rules previousRules
        termNontermSet = nonTerminals ++ terminals
    
checkRightSideSet :: [Rule] -> [NonTerminal] -> [Rule]
checkRightSideSet rules previousRules = [r | r <- rules, leftSide r `elem` previousRules]