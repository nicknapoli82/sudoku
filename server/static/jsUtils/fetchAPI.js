export function fetchAPI(url, cb) {
  function fetchThen() {
    try {
      fetch(url, headers).then(response => response.json().then(data => cb(data)));
    }
    catch (e) {
      cb(null, e);
    }
  }

  const headers = {
    method: 'PUT',
    mode: 'same-origin',
    cache: 'no-cache',
    credentials: 'same-origin',
    referrerPolicy: 'no-referrer',
  };
  return () => fetchThen();
}

export default fetchAPI;
