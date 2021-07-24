const path = require('path');

const PATHS = {
  server: path.dirname(require.main.filename),
  root: path.dirname(require.main.filename).split('/').slice(0, -1).join('/'),
  static: path.dirname(require.main.filename) + '/static'
};

module.exports = PATHS;
