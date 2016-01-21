using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace APISgrai.Models
{
    public class FotoAPIDTO
    {
        public int Imovel_ID { get; set; }
        public string nome { get; set; }
        public string Content { get; set; }
        public string ContentType { get; set; }

        public FotoAPIDTO()
        {
            Imovel_ID = 0;
            nome = "";
            Content = "";
            ContentType = "";
        }
    }
}