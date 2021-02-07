import React from "react";
import { connect } from "react-redux";

import "./styles.sass";

class AnimList extends React.Component {
  constructor(props) {
    super(props)
  }

  getAnimList() {
    return this.props.list.map((anim, index) => (
      <div 
        className="anim"
        key={`anim-${index}`}
      >
        {anim.name}
      </div>
    ))
  }

  render() {
    return (
      this.props.show && (
        <div id="animList">
          {this.getAnimList()}
        </div>
      )
    );
  }
}

const mapStateToProps = (state) => {
  const defaultState = state.animListReducer;
  return {
    show: defaultState.show,
    list: defaultState.list,
  }
}

const mapDispatchToProps = (dispatch) => ({
  updateShow: (data) =>
    dispatch({
      type: "UPDATE_ANIMLIST_SHOW",
      data,
    }),
})

export default connect(mapStateToProps, mapDispatchToProps)(AnimList)