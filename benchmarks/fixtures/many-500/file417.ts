interface Item417 { id: number; name: string; }
type Key417 = string | number;
export function make417(id: number, name: string): Item417 {
  const value: Item417 = {id, name};
  return value as Item417;
}
export const item417: Item417 = make417(417, "item-417");
