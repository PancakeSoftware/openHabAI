import {Component, OnInit, ViewChild} from '@angular/core';
import {Api} from "@catflow/Api";
import {PublishSubject} from "@frontend/util/PublishSubject";

@Component({
  selector: 'app-test-playground',
  templateUrl: './test-playground.component.html',
  styles: []
})
export class TestPlaygroundComponent implements OnInit {

  @ViewChild('editorEl') editor;

  constructor(public api: Api) {

    let s = new PublishSubject<number>();
    s.subscribe(val => console.info('before', val));
    s.next(0);
    s.next(1);
    s.next(20);
    s.subscribe(val => console.info('after', val));
    s.next(30);
  }

  editorReset() {
    this.editor.modelFromJson(this.editor.modelJson);
  }

  ngOnInit() {
    this.editor.networkObject = this.api.object(`/dataStructures/${71}/networks/${0}/`)
  }

}
