////////////////////////////////////////////////////////
// Quite literally the simplest server I can think of //
// as an interface for the Sudoku application	      //
////////////////////////////////////////////////////////

///////////////////////////////////////
// This would be for a hosted server //
// const SSL = require('./sslCert'); //
///////////////////////////////////////

const fs = require('fs');
const path = require('path');
const http = require('http');
const url = require('url');
const { reqAPI } = require('./api.js');

const ROOT = path.resolve('..');
const serveStatic = path.resolve('./static');
const connection = { hostName: 'localhost', port: 3000 };
const server = http.createServer();

server.on('request', (req, res) => {
  console.time(`Serving: ${req.url}  `);
  let document = path.parse(req.url);
  if (document.dir === '/api') {
    reqAPI(req, res, document.base);
    console.timeEnd(`Serving: ${req.url}  `);
    return;
  }
  if (req.url === '/') {
    document.base = 'index.html';
    document.ext = '.html';
  }
  switch (document.ext) {
    case '.html': { res.setHeader('Content-Type', 'text/html'); break; }
    case '.css': { res.setHeader('Content-Type', 'text/css'); break; }
    case '.js': { res.setHeader('Content-Type', 'application/javascript'); break; }
    case '.jpg': { res.setHeader('Content-Type', 'image/jpeg'); break; }
    default: { res.setHeader('Content-Type', 'text/plain'); break; }
  }
  try {
    document.result = fs.readFileSync(path.join(serveStatic, document.base));
    res.statusCode = 200;
    console.timeEnd(`Serving: ${req.url}  `);
    res.end(document.result);
  }
  catch (e) {
    res.statusCode = 404;
    console.timeEnd(`Serving: ${req.url}  `);
    console.log(`Failed!!! ${e}`);
    res.end(e.toString());
  }
});

server.listen(connection.port, connection.hostName, () => {
  console.log(`Server running at https://${connection.hostName}:${connection.port}`);
});
