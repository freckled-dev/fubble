import { Component, OnInit } from '@angular/core';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {

  constructor(  private route: ActivatedRoute,
    private title: Title,
    private meta: Meta) { }

  ngOnInit() {
    this.title.setTitle( this.route.snapshot.data['title']);
    this.meta.updateTag({name: 'description', content: this.route.snapshot.data['content']})
    this.meta.addTag({name: this.route.snapshot.data['title'], content: this.route.snapshot.data['content']})
    this.meta.updateTag({property:"og:title",content: this.route.snapshot.data['content']})
  }
  

}
