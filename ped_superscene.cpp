


Ped::Tsuperscene::Tsuperscene(double left, double top, double width, double height,
                              int iters, int h) : Tsuperscene() {
  //init MPI
  int process_count;
  int process_id;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &process_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  bool is_master = process_id == 0;

  //determine own boundaries (horizontal bands)
  double my_height = height/process_count;
  double my_top = my_height*process_id;
  //create scene
  Tscene(left, my_top, width, my_height);

}
