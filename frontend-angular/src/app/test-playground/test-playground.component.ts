import {Component, OnInit, ViewChild} from '@angular/core';
import {Api} from "@catflow/Api";

@Component({
  selector: 'app-test-playground',
  templateUrl: './test-playground.component.html',
  styles: []
})
export class TestPlaygroundComponent implements OnInit {

  @ViewChild('editorEl') editor;

  constructor(public api: Api) {

  }

  editorReset() {
    this.editor.modelFromJson(this.editor.modelJson);
  }

  ngOnInit() {
    this.editor.networkObject = this.api.object(`/dataStructures/${71}/networks/${0}/`)
  }

}
