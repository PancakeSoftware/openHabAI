import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-progress-indicator',
  template: `
      <div class="spinner">
          <div class="rect1"></div>
          <div class="rect2"></div>
          <div class="rect3"></div>
          <div class="rect4"></div>
          <div class="rect5"></div>
      </div>
  `,
  styleUrls: ['progress-indicator.component.css']
})
export class ProgressIndicatorComponent implements OnInit {

  constructor() { }

  ngOnInit() {
  }

}
