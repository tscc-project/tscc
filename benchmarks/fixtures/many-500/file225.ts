interface Item225 { id: number; name: string; }
type Key225 = string | number;
export function make225(id: number, name: string): Item225 {
  const value: Item225 = {id, name};
  return value as Item225;
}
export const item225: Item225 = make225(225, "item-225");
