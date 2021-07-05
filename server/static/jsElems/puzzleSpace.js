class Tile {
  constructor(n = 0) {
    this.n = n;
    this.elem = document.createElement('div');
    this.elem.className = 'tile';
    this.elem.innerText = '0';
  }
  updateNumber(n) {
    this.elem.width = spaceWH * 0.11;
    this.elem.height = n * spaceWH * 0.11;
  }
  adjustWidthHeight(w) {

  }
}

const puzzleSpace = document.getElementById("puzzle-space");
let spaceWH = window.innerWidth * 0.6 * 0.11;
let puzzleTiles = new Array(81);
for (let t = 0; t < 81; t++) { puzzleTiles[t] = new Tile(0); puzzleSpace.appendChild(puzzleTiles[t].elem); }
console.log(spaceWH * 0.11);

export default { puzzleSpace, puzzleTiles };
