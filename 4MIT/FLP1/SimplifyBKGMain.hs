-- simplify-bkg Main
-- FLP Functional Project
-- Michal Ormos (xormos00)
-- xormos00@stud.fit.vutbr.cz
-- February 2019

module Main where

import System.Environment
import System.IO
import System.Exit

-- import modules with data type and core functionality
import SimplifyBKGCore
import SimplifyBKGData

-- process the file to the program
main :: IO ()
main = do
    args <- getArgs
    let (option, input) = processOptions args
    content <- parseInput input
    grammar <- eliminateRulesBKG option content
    print grammar
    return ()

-- determine arguments
eliminateRulesBKG :: Int -> String -> IO BKG
eliminateRulesBKG parsedArg content
    | parsedArg==0 = getBKG content
    | parsedArg==1 = step1 content
    | parsedArg==2 = step2 content

-- read file as stream fo cahrs and parse it to IO with genContents
parseInput :: String -> IO String
parseInput [] = getContents
parseInput x = readFile x

-- processing of command line arguments
processOptions :: [String] -> (Int,String)
processOptions [] = error "[Error] Input expect one or two arguments -i with combination of -1/-2 see README for more information"
processOptions [arg] = processOptions [arg, ""]
processOptions [arg,parsedArg]
    | arg=="-i" = (0, parsedArg)
    | arg=="-1" = (1, parsedArg)
    | arg=="-2" = (2, parsedArg)
    | otherwise = error "[Error] Unknown argument"
processOptions _ = error "[Error] Expects 2 arguments"