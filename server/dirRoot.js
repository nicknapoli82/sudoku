const path = require('path');

const PATHS = () => {
  if (process.platform === "linux")
 	  return { server: path.dirname(require.main.filename),
	           root: path.dirname(require.main.filename).split('/').slice(0, -1).join('/'),
		         static: path.dirname(require.main.filename) + '/static'
	         };
  else if(process.platform === "win32")
	  return { server: path.dirname(require.main.filename),
		         root: path.dirname(require.main.filename).split('\\').slice(0, -1).join('/'),
		         static: path.dirname(require.main.filename) + '\\static'
	         };
};

module.exports = PATHS();
