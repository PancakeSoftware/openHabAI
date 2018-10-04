import {
  AfterContentChecked, AfterViewChecked,
  AfterViewInit,
  Component,
  ElementRef,
  EventEmitter,
  Input, OnDestroy,
  OnInit,
  Output,
  ViewChild
} from '@angular/core';
import {Drag} from "@frontend/test-playground/model-editor/Drag";
import {
  ModelEditorConnectionComponent,
  pointAdd
} from "@frontend/test-playground/model-editor/model-editor-connection/model-editor-connection.component";
import {ModelEditorComponent} from "@frontend/test-playground/model-editor/model-editor.component";
import {intersectsWithRect, Point} from "@frontend/util/Helper";
import {type} from "os";

@Component({
  selector: 'app-model-editor-node',
  templateUrl: './model-editor-node.component.html',
  styleUrls: ['./model-editor-node.component.scss']
})
export class ModelEditorNodeComponent implements OnInit, AfterViewInit, OnDestroy {

  /* Model operation type and params
   */
  public operationType: string = "?";
  public operationLayerName: string = "";
  public operationParams: OperationParam[] = [];

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
    if (!this.inPort)
      return false;
    return intersectsWithRect(pos, {
      x: this.pos.x + this.inPort.nativeElement.offsetLeft,
      y: this.pos.y +this.inPort.nativeElement.offsetTop
    }, {
      x: this.pos.x + this.inPort.nativeElement.offsetLeft + this.inPort.nativeElement.offsetWidth,
      y: this.pos.y +this.inPort.nativeElement.offsetTop +this.inPort.nativeElement.offsetHeight
    });
  }



  calcPortOffsets() {
    if (this.inPort)
      this.inPortOffset = {
        x: this.inPort.nativeElement.offsetLeft + this.inPort.nativeElement.offsetWidth/2,
        y: this.inPort.nativeElement.offsetTop + this.inPort.nativeElement.offsetHeight/2
      };
    if (this.outPort)
      this.outPortOffset = {
        x: this.outPort.nativeElement.offsetLeft + this.outPort.nativeElement.offsetWidth/2,
        y: this.outPort.nativeElement.offsetTop + this.outPort.nativeElement.offsetHeight/2
      };
  }

  eventPrevent(event) {
    event.stopPropagation();
  }

  drag: Drag;
  gotSize = false;
  visibleObserver;

  ngAfterViewInit() {
    this.calcPortOffsets();

    /**
     * when element is visible
     * => reCalc port offests
     */
    this.visibleObserver = new IntersectionObserver((entries) => {
      if (entries[0].intersectionRatio) {
        this.calcPortOffsets();
        this.refreshConnections();
        // @TODO ugly workaround for waiting until params are rendered
        setTimeout(() => {
          this.calcPortOffsets();
          this.refreshConnections();
        },0);
        this.visibleObserver.unobserve(this.el.nativeElement);
      }
    }, {
      root: document.body
    });
    this.visibleObserver.observe(this.el.nativeElement);


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

      this.setPosition({x: px, y: py});

      this.refreshConnections();
    });
    this.refreshConnections();
  }

  setPosition(pos: Point) {
    this.pos = pos;
    this.el.nativeElement.style.transform = `translate(${pos.x}px, ${pos.y}px)`;
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


  /**
   * model operation types
   */
  setOperationType(type: string) {
    this.operationType = type;
    switch (type)
    {
      case "FullyConnected":
        this.operationParams = [
          {type: "number", name: "num_hidden", discr: "size", value:  "10"},
          {type: "text", name: "activationFunction", value: "tanh", select: ["tanh", "relu", "none"]}
        ];
        break;

      case "Activation":
        this.operationParams = [
          {type: "text", name: "activationFunction", value: "tanh", select: ["tanh", "relu"]}
        ];
        break;

      case "Input":
        this.operationParams = [];
        break;
      case "Label":
        this.operationParams = [
          {type: "text", name: "lossFunction", discr: "loss function", value: "LinearRegressionOutput", select: ["LinearRegressionOutput", "MAERegressionOutput", "LogisticRegressionOutput", "SoftmaxOutput"]}
        ];
        break;
    }
  }

  getOperationType(): string {
    return this.operationType;
  }


  toJson() {
    return {
      operation: this.operationType,
      name:   this.operationLayerName,
      params: this.operationParams.reduce((obj, param) => {obj[param.name] = param.value; return obj;}, {}),
      inputNodes: this.inputConnections.map((con) => this.editor.nodes.indexOf(con.inNode)),
      editorPosition: [this.pos.x, this.pos.y]
    }
  }

  fromJson(json: any) {
    this.operationLayerName = json.name;
    for (let p in json.params) {
      this.operationParams.filter((param) => param.name == p)[0].value = json.params[p];
    }
    this.setPosition({x: json.editorPosition[0], y: json.editorPosition[1]});
  }

  ngOnDestroy() {
    this.drag.destroy();
    this.visibleObserver.unobserve(this.el.nativeElement);
  }
}

interface OperationParam<T = any> {
  type: string;
  name: string;
  discr?: string;
  value: T;
  select?: T[];
}
