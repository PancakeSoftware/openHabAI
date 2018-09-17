import {AfterViewInit, Component, EventEmitter, Input, OnDestroy, OnInit, Output, ViewChild} from '@angular/core';
import {ModelEditorNodeComponent} from "@frontend/test-playground/model-editor/model-editor-node/model-editor-node.component";
import {Drag} from "@frontend/test-playground/model-editor/Drag";
import {Point} from "@frontend/util/Helper";
import {ModelEditorComponent} from "@frontend/test-playground/model-editor/model-editor.component";

@Component({
  selector: 'app-model-editor-connection',
  templateUrl: './model-editor-connection.component.html',
  styles: [`
    :host {
        overflow: visible;
        position: absolute;
        top: 0; left: 0;
        pointer-events: none;
    }
  `]
})
export class ModelEditorConnectionComponent implements OnInit, AfterViewInit, OnDestroy {

  @ViewChild('svg') svg;
  static tension = 0.5;
  dragOutput: Drag;
  static dragging: boolean = false;
  @Input() editor: ModelEditorComponent;
  @Input() inPoint: Point = {x:1, y:2};
  @Input() outPoint: Point = {x:100, y:20};
  @Input() inNode: ModelEditorNodeComponent;
  @Input() outNode: ModelEditorNodeComponent;
  @Output() onRemove = new EventEmitter();

  static lineWidth: number = 4;

  constructor() {
    // drag output
    this.dragOutput = new Drag(null, (x, y) => {
      this.outPoint = {
        x: x + this.dragOutput.elPos[0],
        y: y + this.dragOutput.elPos[1]
      };
      this.refreshView();
    },
     null, null, () => {

      // collision check
      let found:ModelEditorNodeComponent = null;
      for (let node of this.editor.nodes) {
        if (node.inputPortIntersectsWithPos(this.outPoint))
          found = node;
      }
      if (found)
        found.addInputConnection(this);
      else
        this.remove();

      ModelEditorConnectionComponent.dragging = false;
    });

  }

  ngOnInit() {
  }

  ngAfterViewInit() {
    this.refreshView();
  }

  remove() {
    if (this.inNode)
      this.inNode.removeOutputConnection(this);
    if (this.outNode)
      this.outNode.removeInputConnection(this);
    this.onRemove.next();
  }

  outputDrag(posStart: number[], elementPos: number[]) {
    this.dragOutput.elPos = elementPos;
    ModelEditorConnectionComponent.dragging = true;
    this.dragOutput.startDrag(posStart);

    // init pos
    this.outPoint = {
      x: elementPos[0],
      y: elementPos[1]};
    this.refreshView();
  }

  refreshView() {
    let minDistanceToNode = 15;
    let x1 = this.inPoint.x;
    let y1 = this.inPoint.y;
    let x2 = this.outPoint.x;
    let y2 = this.outPoint.y;
    let deltaX = Math.round((x2-x1)*ModelEditorConnectionComponent.tension);
    let deltaY = Math.round((y2-y1)*ModelEditorConnectionComponent.tension);
    let path;

    if (y2-minDistanceToNode > y1+minDistanceToNode) {
      path = deltaY != 0 ?
        `<path d="M ${x1} ${y1} L ${x1} ${y1 + deltaY} ${x2} ${y1 + deltaY}  ${x2} ${y2}" fill="none" stroke="#ffffff" stroke-width="${ModelEditorConnectionComponent.lineWidth}" stroke-linejoin="round" />`
        : `<path d="M ${x1} ${y1}                                            ${x2} ${y2}" fill="none" stroke="#ffffff" stroke-width="${ModelEditorConnectionComponent.lineWidth}"/>`;
    } else {
      path = `<path d="M ${x1} ${y1} L ${x1} ${y1+minDistanceToNode}  ${x1+deltaX} ${y1+minDistanceToNode}  ${x1+deltaX} ${y2-minDistanceToNode}  ${x2} ${y2-minDistanceToNode}   ${x2} ${y2}" 
                fill="none" stroke="#ffffff" stroke-width="${ModelEditorConnectionComponent.lineWidth}" stroke-linejoin="round" />`
    }
    // <path style="filter:url(#dropshadow)" d="M ${x1} ${y1} L ${x1} ${y1+deltaY} ${x2} ${y1+deltaY}   ${x2} ${y2}" fill="none" stroke="#ffffff" stroke-width="4"/>

    this.svg.nativeElement.innerHTML = `
    <svg style="overflow: visible">
        <filter id="dropshadow" height="130%">
          <feGaussianBlur in="SourceAlpha" stdDeviation="3"/> <!-- stdDeviation is how much to blur -->
          <feOffset dx="0" dy="0" result="offsetblur"/> <!-- how much to offset -->
          <feComponentTransfer>
            <feFuncA type="linear" slope="0.5"/> <!-- slope is the opacity of the shadow -->
          </feComponentTransfer>
          <feMerge> 
            <feMergeNode/> <!-- this contains the offset blurred image -->
            <feMergeNode in="SourceGraphic"/> <!-- this contains the element that the filter is applied to -->
          </feMerge>
        </filter>
        ${path}
    </svg>`;
  }

  ngOnDestroy() {
    this.dragOutput.destroy();
  }
}


export function pointAdd(a: Point, b:Point):Point {
  return {x: a.x+b.x, y:a.y+b.y};
}