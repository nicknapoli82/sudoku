function KeyManager() {
  this.keySet = [];
  document.addEventListener('keydown', this.keyDown.bind(this));
};

KeyManager.prototype.registerKeys = function(fun, ...keys) {
  this.keySet = [{ fun, keys: new Set(keys) }, ...this.keySet];
};

KeyManager.prototype.deregisterKeys = function(fun) {
  this.keySet = this.keySet.filter((e) => e.fun !== fun);
};

KeyManager.prototype.keyDown = function(k) {
  for (let i = 0; i < this.keySet.length; i++) {
    if (this.keySet[i].keys.has(k.key)) {
      k.preventDefault();
      this.keySet[i].fun(k);
    }
  }
};

export const keyManager = new KeyManager();

export default { keyManager };
