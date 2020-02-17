import { Component, OnInit } from '@angular/core';
import {Meta, Title} from '@angular/platform-browser';
import {ActivatedRoute} from '@angular/router';

@Component({
  selector: 'app-blog-details-rightside',
  templateUrl: './blog-details-rightside.component.html',
  styleUrls: ['./blog-details-rightside.component.scss']
})
export class BlogDetailsRightsideComponent implements OnInit {

  constructor(private route: ActivatedRoute,
    private title: Title,
    private meta: Meta) { }

  ngOnInit() {
    this.title.setTitle( this.route.snapshot.data['title']);
  }

}
