import {AfterViewInit, Component, ElementRef, EventEmitter, Input, OnInit, Output, ViewChild} from '@angular/core';
import {Drag} from "@frontend/test-playground/model-editor/Drag";
import {
  ModelEditorConnectionComponent,
  pointAdd
} from "@frontend/test-playground/model-editor/model-editor-connection/model-editor-connection.component";
import {ModelEditorComponent} from "@frontend/test-playground/model-editor/model-editor.component";
import {intersectsWithRect, Point} from "@frontend/util/Helper";

@Component({
  selector: 'app-model-editor-node',
  templateUrl: './model-editor-node.component.html',
  styleUrls: ['./model-editor-node.component.scss']
})
export class ModelEditorNodeComponent implements OnInit, AfterViewInit {

  @Input() editor: ModelEditorComponent;
  @Input() editorPanel;
  @Input() index = -1;
  @Output() onRemove = new EventEmitter();

  @ViewChild('box') box;
  @ViewChild('inPort') inPort;
  @ViewChild('outPort') outPort;

   pos: Point = {x:0, y:0};
  private inPortOffset: Point = {x:0,y:0};
  private outPortOffset: Point = {x:0,y:0};
  private inputConnections: ModelEditorConnectionComponent[] = [];
  private outputConnections: ModelEditorConnectionComponent[] = [];

  constructor(
    public el: ElementRef) {
  }

  ngOnInit() {
  }

  addInputConnection(connection: ModelEditorConnectionComponent) {
    connection.outNode = this;
    this.inputConnections.push(connection);
    // refresh
    connection.outPoint = pointAdd(this.pos, this.inPortOffset);
    connection.refreshView();
  }

  removeInputConnection(connection: ModelEditorConnectionComponent) {
    this.inputConnections = this.inputConnections.filter((con) => con !== connection);
  }

  addOutputConnection(connection: ModelEditorConnectionComponent) {
    connection.inNode = this;
    this.outputConnections.push(connection);
    // refresh
    connection.inPoint = pointAdd(this.pos, this.outPortOffset);
    connection.refreshView();
  }

  removeOutputConnection(connection: ModelEditorConnectionComponent) {
    this.outputConnections = this.outputConnections.filter((con) => con !== connection);
  }

  remove() {
    for (let con of this.inputConnections)
      con.remove();
    for (let con of this.outputConnections)
      con.remove();
    this.onRemove.next();
  }

  portInRemove(event) {
    if (ModelEditorConnectionComponent.dragging)
      return;
    event.stopPropagation();
    for (let con of this.inputConnections)
      con.remove();
  }

  portOutAdd(event) {
    event.stopPropagation();

    let connection = this.editor.createConnection();
    this.addOutputConnection(connection);
    connection.outputDrag(
      [event.pageX, event.pageY],
      [this.pos.x+this.outPortOffset.x, this.pos.y + this.outPortOffset.y]);
  }

  inputPortIntersectsWithPos(pos: Point):boolean {
    return intersectsWithRect(pos, {
      x: this.pos.x + this.inPort.nativeElement.offsetLeft,
      y: this.pos.y +this.inPort.nativeElement.offsetTop
    }, {
      x: this.pos.x + this.inPort.nativeElement.offsetLeft + this.inPort.nativeElement.offsetWidth,
      y: this.pos.y +this.inPort.nativeElement.offsetTop +this.inPort.nativeElement.offsetHeight
    });
  }



  calcPortOffsets() {
    this.inPortOffset = {
      x: this.inPort.nativeElement.offsetLeft + this.inPort.nativeElement.offsetWidth/2,
      y: this.inPort.nativeElement.offsetTop + this.inPort.nativeElement.offsetHeight/2
    };
    this.outPortOffset = {
      x: this.outPort.nativeElement.offsetLeft + this.outPort.nativeElement.offsetWidth/2,
      y: this.outPort.nativeElement.offsetTop + this.outPort.nativeElement.offsetHeight/2
    };
  }

  drag: Drag;
  gotSize = false;
  visibleObersver;

  ngAfterViewInit() {
    this.calcPortOffsets();

    /**
     * when element is visible
     * => reCalc port offests
     */
    this.visibleObersver = new IntersectionObserver((entries) => {
      if (entries[0].intersectionRatio) {
        this.calcPortOffsets();
        this.refreshConnections();
        this.visibleObersver.unobserve(this.el.nativeElement);
      }
    }, {
      root: document.body
    });
    this.visibleObersver.observe(this.el.nativeElement);


    /**
     * drag and drop
     */
    let grid = 20;
    /*min step = 10*/
    let height = 300, width = 300;

    this.drag = new Drag(this.el.nativeElement, (x, y, e) => {
      //console.log("drag");
      let px = Math.round((this.drag.elPos[0] + x) / grid) * grid;
      let py = Math.round((this.drag.elPos[1] + y) / grid) * grid;

      // check bounds
      height = this.editorPanel.nativeElement.offsetHeight;
      width = this.editorPanel.nativeElement.offsetWidth;
      px = px < 0 ? 0 : px;
      px = px > width - this.box.nativeElement.offsetWidth ? width - this.box.nativeElement.offsetWidth : px;
      py = py < 0 ? 0 : py;
      py = py > height - this.box.nativeElement.offsetHeight ? height - this.box.nativeElement.offsetHeight : py;

      //console.log(`refreshView(${px}px, ${py}px),  ${x}, ${y}`);
      this.el.nativeElement.style.transform = `translate(${px}px, ${py}px)`;
      this.pos = {x:px, y:py};

      this.refreshConnections();
    });
    this.refreshConnections();
  }

  refreshConnections() {
    // update connections
    for (let con of this.inputConnections) {
      con.outPoint = pointAdd(this.pos, this.inPortOffset);
      con.refreshView();
    }
    for (let con of this.outputConnections) {
      con.inPoint = pointAdd(this.pos, this.outPortOffset);
      con.refreshView();
    }
  }

}
