const path = require('path');

const PATHS = {
  server: require.main.path,
  root: require.main.path.split('/').slice(0, -1).join('/'),
  static: require.main.path + '/static'
};

module.exports = PATHS;
