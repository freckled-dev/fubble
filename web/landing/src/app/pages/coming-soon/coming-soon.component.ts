import { Component, OnInit } from '@angular/core';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';
declare var $: any;

@Component({
  selector: 'app-coming-soon',
  templateUrl: './coming-soon.component.html',
  styleUrls: ['./coming-soon.component.scss']
})
export class ComingSoonComponent implements OnInit {

  constructor(private route: ActivatedRoute,
    private title: Title,
    private meta: Meta) { }

  ngOnInit() {
    $.getScript('./assets/js/timer.js');
    this.title.setTitle( this.route.snapshot.data['title']);
  }

}
