interface Item142 { id: number; name: string; }
type Key142 = string | number;
export function make142(id: number, name: string): Item142 {
  const value: Item142 = {id, name};
  return value as Item142;
}
export const item142: Item142 = make142(142, "item-142");
