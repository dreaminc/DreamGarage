var dreamos;

if (!dreamos) {
    dreamos = {};
}

(function() {
  dreamos.myfunc = function() {
      native function myfunc();
      return myfunc();
  }
})();