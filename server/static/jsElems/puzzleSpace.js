import { keyManager } from '../jsUtils/keyManager.js';

class Tile {
  constructor(y, x, n = 0) {
    this.x = x;
    this.y = y;
    this.n = n;
    this.elem = document.createElement('div');
    this.elem.className = 'tile';
    this.elem.id = `tile${y}${x}`;
    this.elem.innerText = '-';
    this.elem.tabIndex = 0;
  }
  makeActive = () => {
    this.elem.classList.add('tile-input');
    this.elem.focus();
    return this;
  }

  makeInactive = () => {
    this.elem.classList.remove('tile-input');
    return this;
  }

  markInvalid = () => {
    this.elem.classList.add('tile-invalid');
  }

  removeInvalid = () => {
    this.elem.classList.remove('tile-invalid');
  }

  getInput = (key) => {
    this.n = key;
    this.elem.innerText = key;
  }
}

const PuzzleSpace = () => {
  const elem = document.getElementById("puzzle-space");
  let tiles = new Array(9);
  for (let i = 0; i < tiles.length; i++) {
    tiles[i] = new Array(9);
    for (let j = 0; j < tiles[i].length; j++) {
      tiles[i][j] = new Tile(i, j);
      elem.appendChild(tiles[i][j].elem);
    }
  }
  let activeTile = tiles[0][0].makeActive();

  function validRowCol(num, was) {
    const ts = [];
    const ws = [];
    const { x, y } = activeTile;
    for (let yt = 0; yt < 9; yt++) {
      if (tiles[yt][x].n == num)
        ts.push(tiles[yt][x]);
      if (tiles[yt][x].n == was)
        ws.push(tiles[yt][x]);
    }
    for (let xt = 0; xt < 9; xt++) {
      if (tiles[y][xt].n == num)
        ts.push(tiles[y][xt]);
      if (tiles[y][xt].n == was)
        ws.push(tiles[y][xt]);
    }
    if (ts.length > 2) {
      for (let t of ts) {
        t.markInvalid();
      }
    }
    else {
      for (let t of ts) {
        t.removeInvalid();
      }
      for (let w of ws) {
        w.removeInvalid();
      }
    }
  }

  function tilesToString() {
    let result = '';
    for (let i = 0; i < tiles.length; i++) {
      for (let j = 0; j < tiles[i].length; j++) {
        result += tiles[i][j].n;
      }
    }
    return result;
  }

  function numKeyInput(key) {
    let { x, y } = activeTile;
    let was = activeTile.n;
    activeTile.getInput(key.key);
    x++;
    if (x === 9 && y === 8) { x = 0; y = 0; };
    if (x === 9) { x = 0; y++; };
    activeTile.makeInactive();
    validRowCol(key.key, was);
    activeTile = tiles[y][x];
    activeTile.makeActive();
  }

  function tileClicked(e) {
    if (e.target.id.slice(0, 4) === 'tile') {
      let [y, x] = e.target.id.slice(4).split('');
      activeTile.makeInactive();
      activeTile = tiles[y][x];
      activeTile.makeActive();
    }
  }

  function tileArrowKey(key) {
    let { x, y } = activeTile;
    switch (key.key) {
      case 'ArrowUp': { y--; break; }
      case 'ArrowDown': { y++; break; }
      case 'ArrowLeft': { x--; break; }
      case 'ArrowRight': { x++; break; }
      case 'Tab': {
        x += key.shiftKey ? -1 : 1;
        if (x == 9 && y == 8) { y = 0; x = 0; }
        if (x == -1 && y == 0) { y = 8; x = 8; }
        if (x == 9) { y++; x = 0; }
        if (x == -1) { y--; x = 8; }
        break;
      }
    }
    if (x >= 0 && x < 9 && y >= 0 && y < 9) {
      activeTile.makeInactive();
      activeTile = tiles[y][x];
      activeTile.makeActive();
    }
  }

  keyManager.registerKeys(numKeyInput, '1', '2', '3', '4', '5', '6', '7', '8', '9');
  keyManager.registerKeys(tileArrowKey, 'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'Tab');
  document.addEventListener('click', tileClicked);
  return { tilesToString };
};

export const puzzleSpace = PuzzleSpace();

export default { puzzleSpace };
