import fetchAPI from './jsUtils/fetchAPI.js';
import compile from './jsElems/compile.js';
import puzzle from './jsElems/puzzleSpace.js';
import puzzleInput from './jsElems/puzzleInput.js';

// const compileButton = document.getElementById('compile');
// const compileResults = document.getElementById('compile-results');

// // Very basic fetcher based on what element is making the request
// function fetchAPI(element) {
//   function fetchThen(url, hdrs, cb) {
//     try {
//       fetch(url, hdrs).then(response => response.json().then(data => cb(data)));
//     }
//     catch (e) {
//       cb(null, e);
//     }
//   }

//   const headers = {
//     method: 'PUT',
//     mode: 'same-origin',
//     cache: 'no-cache',
//     credentials: 'same-origin',
//     referrerPolicy: 'no-referrer',
//   };
//   switch (element.id) {
//     case 'compile': { fetchThen('/api/compile', headers, compileRes); break; }
//   }
// }

// function compileRes(data, error = null) {
//   compileResults.innerHTML = `<pre>${error || data.stderr || 'Success'}</pre>`;
// }
