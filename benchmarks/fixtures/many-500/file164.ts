interface Item164 { id: number; name: string; }
type Key164 = string | number;
export function make164(id: number, name: string): Item164 {
  const value: Item164 = {id, name};
  return value as Item164;
}
export const item164: Item164 = make164(164, "item-164");
