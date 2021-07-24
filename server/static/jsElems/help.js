import fetchAPI from '../jsUtils/fetchAPI.js';

export const helpButton = document.getElementById('help-button');

export let help = document.createElement('div');
const getHelpFragment = fetchAPI('helpFragment.html', 'TEXT', helpGet);
let helpRX = false;
function helpGet(data, error = null) {
  if (error)
    help.innerText = error;
  else
    help.innerHTML = data;
}

function hideHelp() {
  help.removeEventListener('click', hideHelp);
  document.body.removeChild(help);
}

helpButton.onclick = () => {
  if (!helpRX) {
    getHelpFragment();
    helpRX = true;
  }
  help.classList.add('help');

  document.body.appendChild(help);
  help.addEventListener('click', hideHelp);
};



export default help;
