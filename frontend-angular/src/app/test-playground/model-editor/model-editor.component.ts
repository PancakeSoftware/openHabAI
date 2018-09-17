import {
  AfterViewInit,
  Component,
  ComponentFactory,
  ComponentFactoryResolver, ElementRef,
  HostListener, Input, OnDestroy,
  OnInit,
  ViewChild, ViewContainerRef
} from '@angular/core';
import {Drag} from "@frontend/test-playground/model-editor/Drag";
import {ModelEditorNodeComponent} from "@frontend/test-playground/model-editor/model-editor-node/model-editor-node.component";
import {ModelEditorConnectionComponent} from "@frontend/test-playground/model-editor/model-editor-connection/model-editor-connection.component";
import {ApiObject} from "@catflow/ApiObject";
import {toastErr, toastOk} from "@frontend/util/Log";
import {componentDestroyed} from "ng2-rx-componentdestroyed";
import {Observable, Subject} from "rxjs";

@Component({
  selector: 'app-model-editor',
  templateUrl: './model-editor.component.html',
  styleUrls: ['./model-editor.component.scss']
})
export class ModelEditorComponent implements OnInit, AfterViewInit, OnDestroy {

  networkObj: ApiObject<any>;

  @ViewChild('editorPanel')                       editorPanelEl;
  @ViewChild('nodesEl', { read: ViewContainerRef }) nodesTemplateEl;
  @ViewChild('connectionsEl', { read: ViewContainerRef }) connectionsTemplateEl;

  nodes:        ModelEditorNodeComponent[] = [];
  connections:  ModelEditorConnectionComponent[] = [];
  modelJson: any[] = [];

  constructor(
    private el: ElementRef,
    private resolver: ComponentFactoryResolver) {
  }

  ngOnInit() {
  }

  createNode(operationType: string = "FullyConnected"): ModelEditorNodeComponent {
    //this.container.clear();
    const factory: ComponentFactory<ModelEditorNodeComponent> = this.resolver.resolveComponentFactory(ModelEditorNodeComponent);

    let componentRef = this.nodesTemplateEl.createComponent(factory);
    let node = componentRef.instance;

    node.editorPanel = this.editorPanelEl;
    let index = this.nodes.push(node) -1;

    node.setOperationType(operationType);
    node.index = index;
    node.editor = this;

    // on remove
    let subscr = node.onRemove.subscribe(() => {
      // remove element
      this.nodes.splice(this.nodes.indexOf(node), 1);
      console.log('remove componentRef', node.index);
      subscr.unsubscribe();
      componentRef.destroy();
    });
    return node;
  }

  createConnection(): ModelEditorConnectionComponent {
    const factory: ComponentFactory<ModelEditorConnectionComponent> = this.resolver.resolveComponentFactory(ModelEditorConnectionComponent);

    let componentRef = this.connectionsTemplateEl.createComponent(factory);
    let connection = componentRef.instance;
    connection.editor = this;
    this.connections.push(connection);

    // on remove
    let subscr = connection.onRemove.subscribe(() => {
      // remove element
      this.connections.splice(this.nodes.indexOf(connection), 1);
      subscr.unsubscribe();
      componentRef.destroy();
    });
    return connection;
  }

  removeAllNodesAndConnections() {
    //console.log(`before remove (${this.nodes.length}) (${this.connections.length})`, this.nodes.map((n) => n.index), this.connections);
    let i = this.nodes.length;
    for (let node = this.nodes[0]; node !== undefined && (i>0); node = this.nodes[0]) {
      node.remove();
      i--;
    }
    i = this.connections.length;
    for (let con; con !== undefined && (i>0); con = this.connections[0]) {
      con.remove();
      i--;
    }
    //setTimeout(() =>  console.log(`after remove (${this.nodes.length}) (${this.connections.length})`, this.nodes.map((n) => n.index), this.connections), 100);
  }

  modelToJson(): any[] {
    let model = [];
    for (let op of this.nodes)
      model.push(op.toJson());
    this.modelJson = model;
    return model;
  }

  modelFromJson(json: any[]) {
    this.removeAllNodesAndConnections();

    if (json != null)
    {
      // nodes
      for (let el of json) {
        let node = this.createNode(el.operation);
        node.fromJson(el);
      }
      // connections
      let i = 0;
      for (let el of json) {
        if (el.inputNodes && el.inputNodes.length > 0) {
          for (let conFrom of el.inputNodes) {
            if (conFrom >= this.nodes.length)
              continue;
            let con = this.createConnection();
            this.nodes[i].addInputConnection(con);
            this.nodes[conFrom].addOutputConnection(con);
          }
        }
        i++;
      }
    }
    // add input, label if not exits
    if (this.nodes.filter((node) => node.getOperationType() == "Input").length == 0) {
      this.createNode("Input").setPosition({x: this.editorPanelEl.nativeElement.offsetWidth/2 - 240/2, y: 20});
      this.inputLabelNeedRePos = true;
    }
    if (this.nodes.filter((node) => node.getOperationType() == "Label").length == 0) {
      this.createNode("Label").setPosition({x: this.editorPanelEl.nativeElement.offsetHeight - 60, y: 20});
      this.inputLabelNeedRePos = true;
    }
  }

  saveModel() {
    this.networkObj.update({
      modelDefinition: this.modelToJson()
    })
      .then(() => toastOk("saved model"))
      .catch((err) => toastErr("can't save model: " + err))

  }


  onRefresh:  Subject<boolean> = new Subject<boolean>();
  inputLabelNeedRePos = false;
  @Input('networkObject')
  set networkObject (obj: ApiObject<any>) {
    this.onRefresh.next(true);
    this.networkObj = obj;

    // load graph
    this.networkObj.object().takeUntil(componentDestroyed(this)).takeUntil(this.onRefresh).subscribe((obj) => {
      if (obj == null)
        return;
      this.modelJson = obj.modelDefinition;
      this.modelFromJson(this.modelJson);
    });
    this.networkObj.subscribe();
  }

  visibleObserver;
  ngAfterViewInit() {
    /**
     * when element is visible
     * => reCalc input, label offest
     */
    this.visibleObserver = new IntersectionObserver((entries) => {
      if (entries[0].intersectionRatio && this.inputLabelNeedRePos) {
        let nodeInput = this.nodes.filter((n) => n.getOperationType() == "Input")[0];
        let nodeLabel = this.nodes.filter((n) => n.getOperationType() == "Label")[0];
        if (nodeInput)
          nodeInput.setPosition({x: this.editorPanelEl.nativeElement.offsetWidth/2 - 240/2, y: 20});
        if (nodeLabel)
          nodeLabel.setPosition({x: this.editorPanelEl.nativeElement.offsetWidth/2 - 240/2, y: this.editorPanelEl.nativeElement.offsetHeight - 80});
        this.inputLabelNeedRePos = false;
      }
    }, {
      root: document.body
    });
    this.visibleObserver.observe(this.el.nativeElement);
  }


  ngOnDestroy() {
    this.visibleObserver.unobserve(this.el.nativeElement);
    this.networkObj.unsubscribe();
    this.removeAllNodesAndConnections();
  }
}