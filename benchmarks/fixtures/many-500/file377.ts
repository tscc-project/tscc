interface Item377 { id: number; name: string; }
type Key377 = string | number;
export function make377(id: number, name: string): Item377 {
  const value: Item377 = {id, name};
  return value as Item377;
}
export const item377: Item377 = make377(377, "item-377");
