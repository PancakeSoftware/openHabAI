import {
  AfterViewInit,
  Component,
  ComponentFactory,
  ComponentFactoryResolver,
  HostListener,
  OnInit,
  ViewChild, ViewContainerRef
} from '@angular/core';
import {Drag} from "@frontend/test-playground/model-editor/Drag";
import {ModelEditorNodeComponent} from "@frontend/test-playground/model-editor/model-editor-node/model-editor-node.component";
import {ModelEditorConnectionComponent} from "@frontend/test-playground/model-editor/model-editor-connection/model-editor-connection.component";

@Component({
  selector: 'app-model-editor',
  templateUrl: './model-editor.component.html',
  styleUrls: ['./model-editor.component.scss']
})
export class ModelEditorComponent implements OnInit {

  @ViewChild('editorPanel')                       editorPanelEl;
  @ViewChild('nodes', { read: ViewContainerRef }) nodesTemplateEl;
  @ViewChild('connections', { read: ViewContainerRef }) connectionsTemplateEl;

  nodes:        ModelEditorNodeComponent[] = [];
  connections:  ModelEditorConnectionComponent[] = [];

  constructor(
    private resolver: ComponentFactoryResolver) {
  }

  ngOnInit() {
    let connection = this.createConnection();
    let connection2 = this.createConnection();
    let node0 = this.createNode();

    let node1 = this.createNode();
    node0.addOutputConnection(connection);
    node1.addInputConnection(connection);

    let node2 = this.createNode();
    node1.addOutputConnection(connection2);
    node2.addInputConnection(connection2);

    /*
    let node3 = this.createNode();
    node2.addOutputConnection(connection2);
    node3.addInputConnection(connection2);
*/

    connection = this.createConnection();
    node1.addOutputConnection(connection);
    this.createNode().addInputConnection(connection);

    connection = this.createConnection();
    node2.addOutputConnection(connection);
    this.createNode().addInputConnection(connection);

  }

  createNode(): ModelEditorNodeComponent {
    //this.container.clear();
    const factory: ComponentFactory<ModelEditorNodeComponent> = this.resolver.resolveComponentFactory(ModelEditorNodeComponent);

    let componentRef = this.nodesTemplateEl.createComponent(factory);
    let node = componentRef.instance;

    node.editorPanel = this.editorPanelEl;
    let index = this.nodes.push(node) -1;

    node.index = index;
    node.editor = this;

    // on remove
    node.onRemove.subscribe(() => componentRef.destroy());
    return node;
  }

  createConnection(): ModelEditorConnectionComponent {
    //this.container.clear();
    const factory: ComponentFactory<ModelEditorConnectionComponent> = this.resolver.resolveComponentFactory(ModelEditorConnectionComponent);

    let componentRef = this.connectionsTemplateEl.createComponent(factory);
    let connection = componentRef.instance;
    connection.editor = this;
    this.connections.push(connection);

    // on remove
    connection.onRemove.subscribe(() => componentRef.destroy());
    return connection;
  }

}