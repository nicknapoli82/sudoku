const { execSync } = require('child_process');
const fs = require('fs');

if (!fs.existsSync('./key.pem') || !fs.existsSync('./cert.pem')) {
  execSync('openssl genrsa -out key.pem');
  execSync("echo '.\n.\n.\nSudoku!\n.\n.\n.\n.\n.\n' | openssl req -new -key key.pem -out csr.pem");
  execSync('openssl x509 -req -days 9999 -in csr.pem -signkey key.pem -out cert.pem');
  execSync('rm ./csr.pem');
}

const SSL = {
  key: fs.readFileSync('key.pem'),
  cert: fs.readFileSync('cert.pem')
};

module.exports = SSL;

