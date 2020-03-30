-- univfunc.hs
import Prelude hiding (length, sequence)


pair :: Int -> Int -> Int
pair x y = (x+y) * (x+y+1) `div` 2 + x + 1


left :: Int -> Int
left z | z == 0    = 0
       | otherwise = head [ x | x <- [0..z], y <- [0..z], pair x y == z ]


right :: Int -> Int
right z | z == 0    = 0
        | otherwise = head [ y | x <- [0..z], y <- [0..z], pair x y == z ]


encode :: [Int] -> Int
encode xs | null xs   = 0
          | otherwise = pair (head xs) (encode $ tail xs)


decode :: Int -> [Int]
decode a | a == 0    = []
         | otherwise = (left a) : (decode $ right a)


element :: Int -> Int -> Int
element a i | i == 1    = left a
            | otherwise = element (right a) (i-1)


length :: Int -> Int
length a = foldl (\n _ -> n+1) 0 (decode a)


replace :: Int -> Int -> Int -> Int
replace a i x | i == 1    = pair x (right a)
              | otherwise = pair (left a) (replace (right a) (i-1) x)


sequence :: Int -> Int -> Int
sequence x k | k == 0    = 0
             | otherwise = pair x (sequence x (k-1))
