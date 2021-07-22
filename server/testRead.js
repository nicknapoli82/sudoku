const fs = require('fs');

let test = undefined;
fs.readFile('./testRead.js', 'utf8', (err, data) => {
  if (err)
    console.log(err);
  else {
    test = data;
    console.log(test);
  }
});

console.log(test);

let dir = fs.readdir('./', (err, data) => { console.log(data); });
console.log(dir);
