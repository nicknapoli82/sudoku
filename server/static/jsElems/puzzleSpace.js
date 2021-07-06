class Tile {
  constructor(n = 0) {
    this.n = n;
    this.elem = document.createElement('div');
    this.elem.className = 'tile';
    this.elem.id = `tile${n}`;
    this.elem.innerText = '_';
    this.elem.tabIndex = 0;
    this.elem.addEventListener('focusin', this.makeActive);
    this.elem.addEventListener('focusout', this.makeInactive);
  }
  makeActive = () => {
    this.elem.classList.add('tile-input');
    this.elem.addEventListener('keydown', this.getInput);
    this.elem.focus();
  }

  makeInactive = () => {
    this.elem.classList.remove('tile-input');
    this.elem.removeEventListener('keydown', this.getInput);
  }

  getInput = (key) => {
    if (Number(key.key)) {
      this.n = key;
      this.elem.innerText = key.key;
      let next = Number(activeTile.elem.id.slice(4)) + 1;
      if (next >= 81)
        next = 0;
      console.log(next);
      activeTile = puzzleTiles[next];
      activeTile.elem.focus();
    }
  }
}

export const puzzleSpace = document.getElementById("puzzle-space");

export let puzzleTiles = new Array(81);
for (let t = 0; t < 81; t++) { puzzleTiles[t] = new Tile(t); puzzleSpace.appendChild(puzzleTiles[t].elem); }
let activeTile = puzzleTiles[0];
activeTile.makeActive;

export function tilesToString() {
  return puzzleTiles.reduce((acc, val) => acc + ' ' + (val.n ? val.n : 0), '');
}

console.log(tilesToString());

export default { puzzleSpace, puzzleTiles, tilesToString };
