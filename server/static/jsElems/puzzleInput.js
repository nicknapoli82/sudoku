import { puzzleSpace } from "./puzzleSpace.js";

export const puzzleInput = document.getElementById('puzzle-IO');

export const getInputNums = () => {
  let commonUnset = null;
  const nums = [];
  for (let i = 0; i < puzzleInput.value.length; i++) {
    let n = Number.parseInt(puzzleInput.value[i]);
    if ((!commonUnset && isNaN(n)) || n == 0) {
      commonUnset = puzzleInput.value[i];
    }
    if (n == commonUnset || puzzleInput.value[i] == commonUnset) {
      nums.push('0');
      continue;
    }
    if (n) {
      nums.push(puzzleInput.value[i]);
    }
  }
  if (nums.length < 81) {
    console.log("All bad!!!");
    return 0;
  }


  return { nums, commonUnset };
};

export const setInputTiles = (tiles) => {
  let result = [];
  for (let y = 0; y < 9; y++) {
    for (let x = 0; x < 9; x++) {
      result.push(tiles[y][x].n);
    }
  }
  puzzleInput.value = result.join('');
};



export default { getInputNums, setInputTiles, puzzleInput };
