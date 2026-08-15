interface Item490 { id: number; name: string; }
type Key490 = string | number;
export function make490(id: number, name: string): Item490 {
  const value: Item490 = {id, name};
  return value as Item490;
}
export const item490: Item490 = make490(490, "item-490");
