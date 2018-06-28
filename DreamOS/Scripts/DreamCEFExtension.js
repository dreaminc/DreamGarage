"use strict";

var test;

if (!test) {
    test = {};
}

(function() {
  test.myfunc = function() {
      native function myfunc();
      return myfunc();
  }
})();