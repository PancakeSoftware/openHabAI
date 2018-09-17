export class Drag {

  mouseStart = null;
  el = null;
  public elPos = [];
  onTranslate;
  onStart;
  onDrag;
  onEnd;

  funcDown;
  funcMove;
  funcUp;
  static count = 0;

  constructor(el, onTranslate, onStart = () => {}, onDrag = () => {}, onEnd = () => {}) {
    Drag.count++;
    console.info('Drag: ', Drag.count);
    this.mouseStart = null;

    this.el = el;
    this.onTranslate = onTranslate;
    this.onStart = onStart;
    this.onDrag = onDrag;
    this.onEnd = onEnd;

    this.funcDown = this.down.bind(this);
    this.funcUp = this.up.bind(this);
    this.funcMove = this.move.bind(this);

    if (el)
      el.addEventListener('mousedown', this.funcDown);
    else
      window.addEventListener('mousedown', this.funcDown);
    window.addEventListener('mousemove', this.funcMove);
    window.addEventListener('mouseup', this.funcUp);

    if (el) {
      el.addEventListener('touchstart', this.funcDown); }
    window.addEventListener('touchmove', this.funcMove, false);
    window.addEventListener('touchend', this.funcUp);
  }

  getCoords(e) {
    const props = e.touches ? e.touches[0] : e;
    return [props.pageX, props.pageY];
  }

  down(e) {
    //e.stopPropagation();

    // stop drag on down
    if (!this.el  && this.mouseStart) {
      //e.stopPropagation();
      this.mouseStart = null;
      this.onEnd();
      return;
    }
    else if (!this.el)
      return;

    let values = this.el.style.transform.match(/([-+]?[\d\.]+)/g);
    if (values)
      this.elPos = [Number(values[0]), Number(values[1])];
    else
      this.elPos = [0,0];
    this.mouseStart = this.getCoords(e);

    //console.log("start drag", this.elPos, this.mouseStart);
    if (this.onStart)
      this.onStart(e);
  }

  startDrag(mouseStart: number[]) {
    this.mouseStart = mouseStart;
  }

  move(e) {
    //if (!this.el) console.log(this.el);
    if (!this.mouseStart) return;
    e.preventDefault();
    e.stopPropagation();

    let [x, y] = this.getCoords(e);
    let delta = [x - this.mouseStart[0], y - this.mouseStart[1]];
    let zoom = 1;//this.el.getBoundingClientRect().width / this.el.offsetWidth;
    //console.info(delta);
    this.onTranslate(delta[0] / zoom, delta[1] / zoom, e);
  }

  up(e) {
    if (!this.mouseStart) return;
    if (!this.el) return;

    this.mouseStart = null;
    if (this.onDrag)
      this.onDrag(e);
  }

  destroy() {
    if (this.el)
      this.el.removeEventListener('mousedown', this.funcDown);
    else
      window.removeEventListener('mousedown', this.funcDown);
    window.removeEventListener('mousemove', this.funcMove);
    window.removeEventListener('mouseup', this.funcUp);

    if (this.el) {
      this.el.removeEventListener('touchstart', this.funcDown); }
    window.removeEventListener('touchmove', this.funcMove, false);
    window.removeEventListener('touchend', this.funcUp);

    // cleanup
    this.el = null;
    this.onTranslate = null;
    this.onStart = null;
    this.onDrag = null;
    this.onEnd = null;

    Drag.count--;
  }
}
