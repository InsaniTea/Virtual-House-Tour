using APISgrai.Areas.HelpPage;
using APISgrai.Models;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Http;

namespace APISgrai.Controllers
{
    public class imovel_fotoController : ApiController
    {

        [Route("api/GetImoveisFotosID")]
        [HttpGet]
        public List<int> GetImoveisFotosID()
        {
            string folderName = "C:\\apisgrai\\ImoveisFoto";
            System.IO.Directory.CreateDirectory(folderName);

                HashSet<int> l = new HashSet<int>();
     

            string [] fileEntries = Directory.GetFiles(folderName);
            foreach (string fileName in fileEntries)
            {
                string[] str = fileName.Split('\\');
                string[] name = str.Last().Split('_');
                    
                l.Add(int.Parse(name[0]));
            }


            return l.ToList();
        }

//        [Route("api/GetImoveisFotosID/{id}")]
//        [HttpGet]
//        public List<FotoDTO> GetImoveisFotosByID(int id)
//        {
//            string folderName = "C:\\apisgrai\\ImoveisFoto";
//            System.IO.Directory.CreateDirectory(folderName);

//            List<FotoDTO> l = new List<FotoDTO>();


//            string[] fileEntries = Directory.GetFiles(folderName);
//            foreach (string fileName in fileEntries)
//            {
//                FileInfo fileInfo = new FileInfo(fileName);

//// The byte[] to save the data in
//byte[] data = new byte[fileInfo.Length];

//// Load a filestream and put its content into the byte[]
//using (FileStream fs = fileInfo.OpenRead())
//{
//    fs.Read(data, 0, data.Length);
//}
//// Delete the temporary file
//fileInfo.Delete();

//                string[] str = fileName.Split('\\');
//                string[] name = str.Last().Split('_');

//                if(name[0]==id.ToString())
//                {

//                    FotoDTO tmp = new FotoDTO
//                    {
//                        Imovel_ID = int.Parse(name[0]),
//                        nome = name[1],
//                        ContentType = "jpeg",
//                        Content = System.Text.Encoding.Default.GetString(data)
//                    };

//                    l.Add(tmp);
//                }
//            }


//            return l.ToList();
//        }

        [Route("api/GetImoveisFotosID/{id}")]
        [HttpGet]
        public IEnumerable<FotoAPIDTO> GetImoveisFotosByID(int id)
        {
            string folderName = "C:\\apisgrai\\ImoveisFoto";
            System.IO.Directory.CreateDirectory(folderName);

            List<FotoAPIDTO> l = new List<FotoAPIDTO>();


            string[] fileEntries = Directory.GetFiles(folderName);
            foreach (string fileName in fileEntries)
            {
                FileInfo fileInfo = new FileInfo(fileName);

                // The byte[] to save the data in
                byte[] data = new byte[fileInfo.Length];

                // Load a filestream and put its content into the byte[]
                using (FileStream fs = fileInfo.OpenRead())
                {
                    fs.Read(data, 0, data.Length);
                }


                string[] str = fileName.Split('\\');
                string[] name = str.Last().Split('_');

                if (name[0] == id.ToString())
                {
                    FotoAPIDTO f = new FotoAPIDTO();

                    string path = "https://10.8.11.50:44303/ImoveisFoto/";

                    path += str.Last();

                    f.Content = path;

                    l.Add(f);

                }
            }


            return l;
        }

        // GET api/<controller>/5
        public string Get(int id)
        {
            return "value";
        }

        // POST api/<controller>
        public IHttpActionResult Post(List<FotoDTO> l)
        {
            try
            {
                string folderName = "C:\\apisgrai\\ImoveisFoto";
                System.IO.Directory.CreateDirectory(folderName);

                foreach (FotoDTO f in l)
                {
                    //// The byte[] to save the data in
                    byte[] data = System.Text.Encoding.Default.GetBytes(f.Content);



                    MemoryStream ms = new MemoryStream(data);
                    Image i = Image.FromStream(ms);

                    i.Save(folderName+"\\"+ f.Imovel_ID.ToString() + "_" + f.nome + ".jpg", ImageFormat.Jpeg);


                }
            }
            catch (Exception e)
            {
                return Ok(HelpPageSampleGenerator.UnwrapException(e).Message);
            }

            return Ok();
        }

        // PUT api/<controller>/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE api/<controller>/5
        public void Delete(int id)
        {
        }
    }
}