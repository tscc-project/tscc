interface Item185 { id: number; name: string; }
type Key185 = string | number;
export function make185(id: number, name: string): Item185 {
  const value: Item185 = {id, name};
  return value as Item185;
}
export const item185: Item185 = make185(185, "item-185");
