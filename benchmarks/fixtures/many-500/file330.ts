interface Item330 { id: number; name: string; }
type Key330 = string | number;
export function make330(id: number, name: string): Item330 {
  const value: Item330 = {id, name};
  return value as Item330;
}
export const item330: Item330 = make330(330, "item-330");
