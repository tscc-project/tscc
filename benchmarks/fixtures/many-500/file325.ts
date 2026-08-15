interface Item325 { id: number; name: string; }
type Key325 = string | number;
export function make325(id: number, name: string): Item325 {
  const value: Item325 = {id, name};
  return value as Item325;
}
export const item325: Item325 = make325(325, "item-325");
