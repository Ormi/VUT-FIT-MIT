-- simplify-bkg Data module
-- FLP Functional Project
-- Michal Ormos (xormos00)
-- xormos00@stud.fit.vutbr.cz
-- February 2019

module SimplifyBKGData where

import Data.List

-- type xxx = [Int]
type NonTerminal = String -- <=> [Char]
type Terminal = String -- <=> [Char]
type NonTermTerminal = String

-- Types of Rules
data Rule = Rule { 
    leftSide :: NonTerminal,  -- [Char]
    rightSide :: NonTermTerminal -- x -- [[Char]]
	} deriving (Eq)

-- Context free Grammar (BKG) types
data BKG = BKG { 
    nonTerminals :: [NonTerminal], -- [[Char]]
    terminals :: [Terminal], -- [[Char]]
    startState :: NonTerminal, -- [Char]
    rules :: [Rule]
	} deriving (Eq)    

-- intersperse :: Char -> Text -> Text
instance Show Rule where
    show (Rule left right) = left ++ "->" ++ right --concat (intersperse "" right)

-- intersperse :: Char -> Text -> Text
instance Show BKG where
    show (BKG nonTerminals terminals startState rules) = intercalate "," nonTerminals ++ "\n" ++ intercalate "," terminals ++ "\n" ++ startState ++ "\n" ++ intercalate "\n" (map show rules)