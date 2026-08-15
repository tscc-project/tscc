interface Item480 { id: number; name: string; }
type Key480 = string | number;
export function make480(id: number, name: string): Item480 {
  const value: Item480 = {id, name};
  return value as Item480;
}
export const item480: Item480 = make480(480, "item-480");
