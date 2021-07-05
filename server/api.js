const { exec } = require('child_process');
const path = require('path');

const ROOT = path.resolve('..');

function reqAPI(req, res, route) {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'application/json');
  switch (route) {
    case 'compile': { compileAndRes(res); break; }
  }
}

function compileAndRes(res) {
  exec(`clang -o ${path.join(ROOT, 'sudoku')} ${path.join(ROOT, 'sudoku.c')}`, (error, stdout, stderr) => {
    const result = { error, stdout, stderr };
    res.end(JSON.stringify(result));
  });
}

module.exports = { reqAPI };
