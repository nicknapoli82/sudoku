export function fetchAPI(url, type = 'JSON', cb) {
  function fetchThen() {
    try {
      if (type === 'JSON')
        fetch(url, headers).then(response => response.json().then(data => cb(data)));
      else fetch(url, headers).then(response => response.text().then(text => cb(text)));
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
