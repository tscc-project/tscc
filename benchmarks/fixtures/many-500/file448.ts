interface Item448 { id: number; name: string; }
type Key448 = string | number;
export function make448(id: number, name: string): Item448 {
  const value: Item448 = {id, name};
  return value as Item448;
}
export const item448: Item448 = make448(448, "item-448");
