using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace APISgrai.Models
{
    public class FotoDTO
    {
        public int Imovel_ID { get; set; }
        public string nome { get; set; }
        public string Content { get; set; }
        public string ContentType { get; set; }
    }
}