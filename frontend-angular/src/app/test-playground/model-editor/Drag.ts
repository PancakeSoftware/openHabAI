export class Drag {

  mouseStart = null;
  el = null;
  public elPos = [];
  onTranslate;
  onStart;
  onDrag;
  onEnd;

  constructor(el, onTranslate, onStart = () => {}, onDrag = () => {}, onEnd = () => {}) {
    this.mouseStart = null;

    this.el = el;
    this.onTranslate = onTranslate;
    this.onStart = onStart;
    this.onDrag = onDrag;
    this.onEnd = onEnd;

    if (el)
      el.addEventListener('mousedown', this.down.bind(this));
    else
      window.addEventListener('mousedown', this.down.bind(this));
    window.addEventListener('mousemove', this.move.bind(this));
    window.addEventListener('mouseup', this.up.bind(this));

    if (el) {
      el.addEventListener('touchstart', this.down.bind(this)); }
    window.addEventListener('touchmove', this.move.bind(this), false);
    window.addEventListener('touchend', this.up.bind(this));
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
}
