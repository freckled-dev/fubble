import { Component, OnInit } from '@angular/core';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';

@Component({
  selector: 'app-thank-you',
  templateUrl: './thank-you.component.html',
  styleUrls: ['./thank-you.component.scss']
})
export class ThankYouComponent implements OnInit {

  constructor(private route: ActivatedRoute,
    private title: Title,
    private meta: Meta) { }

  ngOnInit() {
    this.title.setTitle( this.route.snapshot.data['title']);
  }

}
