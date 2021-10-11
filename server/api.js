const { exec } = require('child_process');
const path = require('path');
const fs = require('fs');

const PATHS = require('./dirRoot.js');


function reqAPI(req, res, route) {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'application/json');
  switch (route) {
    case 'compile': { compileAndRes(res); break; }
    case 'getPuzzlesAvailable': { getPuzzlesAvailable(res); break; }
    case 'solvePuzzle': { solvePuzzle(req, res); break; }
  }
}

function compileAndRes(res) {
  exec(`cc -o ${path.join(PATHS.root, 'sudoku')} ${path.join(PATHS.root, 'sudoku.c')}`, (error, stdout, stderr) => {
    const result = { error, stdout, stderr };
    res.end(JSON.stringify(result));
  });
}

function getPuzzlesAvailable(res) {
  const puzzles = [];

  function findPuzzles(current) {
    const puzzleDir = current.split(PATHS.root).pop();
    const puzzlesLoc = puzzles.length;
    puzzles.push({ puzzleDir, files: [] });
    fs.readdir(current, (err, files) => {
      for (let f of files) {
        const fName = `${current}/${f}`;
        fs.lstat(fName, (err, dirCheck) => {
          if (dirCheck.isDirectory()) {
            findPuzzles(fName);
          }
          else {
            const fPosition = puzzles[puzzlesLoc].files.length;
            puzzles[puzzlesLoc].files[fPosition] = { f: f.split('.')[0], data: undefined };
            fs.readFile(fName, 'utf8', (err, data) => {
              if (err) data = err;
              puzzles[puzzlesLoc].files[fPosition].data = data;
              filesComplete();
            });
          }
        });
      }
    });
  }

  function filesComplete() {
    for (const dir of puzzles) {
      for (const f of dir.files) {
        if (!f.data)
          return;
      }
    }
    res.end(JSON.stringify(puzzles));
  }

  findPuzzles(PATHS.root + '/puzzles');
}

function solvePuzzle(req, res) {
  let data = [];
  req.on('data', chunk => {
    data.push(chunk);
  });
  req.on('end', () => {
    data = data.map((b) => JSON.parse(b)).join().split('')
      .filter((c) => !isNaN(Number.parseInt(c))).join('');
    if (data.length < 81)
      res.end(JSON.stringify({ stdout: "Puzzle input is not long enough!" }));
    exec(`echo ${data} | ${PATHS.root}/sudoku`, (error, stdout, stderr) => {
      const result = { error, stdout, stderr };
      res.end(JSON.stringify(result));
    });
  });
}

module.exports = { reqAPI };
