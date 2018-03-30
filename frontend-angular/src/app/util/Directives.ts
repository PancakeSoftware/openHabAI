import {Directive, ElementRef, HostListener} from "@angular/core";

@Directive({
  selector: "[app-click-stop-propagation]"
})
export class ClickStopPropagation
{
  constructor(el: ElementRef) {
    console.log('used directive');
  }

  @HostListener("click", ["$event"])
  public onClick(event: any): void
  {
    event.stopPropagation();
    console.log('stopped');
  }
}