require 'test_helper'

class ZiploadsControllerTest < ActionController::TestCase
  setup do
    @zipload = ziploads(:one)
  end

  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:ziploads)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create zipload" do
    assert_difference('Zipload.count') do
      post :create, :zipload => @zipload.attributes
    end

    assert_redirected_to zipload_path(assigns(:zipload))
  end

  test "should show zipload" do
    get :show, :id => @zipload.to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => @zipload.to_param
    assert_response :success
  end

  test "should update zipload" do
    put :update, :id => @zipload.to_param, :zipload => @zipload.attributes
    assert_redirected_to zipload_path(assigns(:zipload))
  end

  test "should destroy zipload" do
    assert_difference('Zipload.count', -1) do
      delete :destroy, :id => @zipload.to_param
    end

    assert_redirected_to ziploads_path
  end
end
