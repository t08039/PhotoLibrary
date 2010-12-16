PhotoLibrary::Application.routes.draw do
  resources :comments
  
  match 'update_comment/(:id)' => "comments#update_comment", :as => :update_comment

  resources :ziploads

  resources :zips

  resources :albums
  
  match 'show_user_album/(:id)' => "albums#show_user_album", :as => :show_user_album
  
  resources :users

  resource :session, :only => [:new, :create, :destroy]

  match 'signup' => 'users#new', :as => :signup

  match 'register' => 'users#create', :as => :register

  match 'login' => 'sessions#new', :as => :login

  match 'logout' => 'sessions#destroy', :as => :logout

  match '/activate/:activation_code' => 'users#activate', :as => :activate, :activation_code => nil

  resources :photos

  # The priority is based upon order of creation:
  # first created -> highest priority.

  # Sample of regular route:
  #   match 'products/:id' => 'catalog#view'
  # Keep in mind you can assign values other than :controller and :action

  match 'tagsearch/(:id)' => "photos#tagsearch", :as => :tagsearch
  match 'show_user/(:id)' => "photos#show_user", :as => :show_user
  match 'edit_title/(:id)' => "photos#edit_title", :as => :edit_title
  match 'edit_tag/(:id)' => "photos#edit_tag", :as => :edit_tag
  match 'update_title/(:id)' => "photos#update_title", :as => :update_title
  match 'update_tag/(:id)' => "photos#update_tag", :as => :update_tag
  match 'test/(:id)' => "photos#test", :as => :test
  
  
  # Sample of named route:
  #   match 'products/:id/purchase' => 'catalog#purchase', :as => :purchase
  # This route can be invoked with purchase_url(:id => product.id)

  # Sample resource route (maps HTTP verbs to controller actions automatically):
  #   resources :products

  # Sample resource route with options:
  #   resources :products do
  #     member do
  #       get 'short'
  #       post 'toggle'
  #     end
  #
  #     collection do
  #       get 'sold'
  #     end
  #   end

  # Sample resource route with sub-resources:
  #   resources :products do
  #     resources :comments, :sales
  #     resource :seller
  #   end

  # Sample resource route with more complex sub-resources
  #   resources :products do
  #     resources :comments
  #     resources :sales do
  #       get 'recent', :on => :collection
  #     end
  #   end

  # Sample resource route within a namespace:
  #   namespace :admin do
  #     # Directs /admin/products/* to Admin::ProductsController
  #     # (app/controllers/admin/products_controller.rb)
  #     resources :products
  #   end

  # You can have the root of your site routed with "root"
  # just remember to delete public/index.html.
  # root :to => "welcome#index"

  # See how all your routes lay out with "rake routes"

  # This is a legacy wild controller route that's not recommended for RESTful applications.
  # Note: This route will make all actions in every controller accessible via GET requests.
   match ':controller(/:action(/:id(.:format)))'
end
